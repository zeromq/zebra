/*  =========================================================================
    ztrie - URL routing and dispating.

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    ztrie - URL routing and dispating.
@discuss
@end
*/

#include "../include/zwebrap.h"
#include "zwebrap_classes.h"

#define MODE_INSERT 0
#define MODE_MATCH  1
#define MODE_PREFIX 2

#define NODE_TYPE_STRING 0  //  Node with string token
#define NODE_TYPE_REGEX  1  //  Node with regex token
#define NODE_TYPE_PARAM  2  //  Node with regex token and capturing group

#define MIN_LEN(x,y) \
    y + ((x - y) & ((x - y) >>(sizeof(int) * CHAR_BIT - 1)))

//  Trie node, used internally only

typedef struct _ztrie_node_t {
    char *token;         //  Part of an path between to delemiters e.g. '/{token}/'
    int token_type;      //  Type of the token, string or regex
    int token_len;       //  Number of characters in the token
    size_t path_len;     //  Length of the path including this token
    bool endpoint;       //  Has a path been added that routes to this node?
    size_t parameter_count;  //  How many regex parameters does this token contaicontain?
    char **parameter_names;  //  Names of the regex parameters for easy access at matching time
    char **parameter_values; //  Values of the parameters
    zrex_t *regex;           //  Compiled regex
    void *data;              //  Custom arbitrary data assoziated with a route
    ztrie_destroy_data_fn *destroy_data_fn;  // Callback to destroy custom data
    zlistx_t *children;       //  List of all children to this route
    struct _ztrie_node_t *parent;  // Parent of this node
} ztrie_node_t;

//  --------------------------------------------------------------------------
//  Structure of our class

struct _ztrie_t {
    ztrie_node_t *root;     //  Root node of this tree
    ztrie_node_t *match;    //  Last match made by ztrie_matches
    zlistx_t *params;        //  List of regex parameters found during parsing
                            //  The list is kept globally to optimize performance.
};

//  Internal helper functions

static int
s_ztrie_node_compare (const void *item1, const void *item2) {
    assert (item1);
    assert (item2);
    ztrie_node_t *node1 = (ztrie_node_t *) item1;
    ztrie_node_t *node2 = (ztrie_node_t *) item2;

    int same = node1->token_type - node2->token_type;
    if (same == 0) {
        if (*node1->token == *node2->token  //  This achieves a small performance boost
            && node1->token_len == node2->token_len
            && strncmp (node1->token, node2->token, MIN_LEN (node1->token_len, node2->token_len)) == 0)
            return 0;
        else
            return -1;
    }
    else
        return same;
}


//  Create a new ztrie_node. If parent is not NULL the child will add itself
//  to the parents children.

static ztrie_node_t *
s_ztrie_node_new (ztrie_node_t *parent, char *token, int token_len, zlistx_t *param_keys, int token_type)
{
    ztrie_node_t *self = (ztrie_node_t *) zmalloc (sizeof (ztrie_node_t));
    assert (self);

    //  Initialize properties
    self->token = strndup (token, token_len);
    self->token_type = token_type;
    self->token_len = token_len;
    self->endpoint = false;
    self->parameter_count = 0;
    self->parameter_names = NULL;
    self->parameter_values = NULL;
    if (param_keys && zlistx_size (param_keys) > 0) {
        self->parameter_count = zlistx_size (param_keys);
        self->parameter_names = (char **) malloc (sizeof (char *) * self->parameter_count);
        self->parameter_values = (char **) malloc (sizeof (char *) * self->parameter_count);
        char *key = (char *) zlistx_first (param_keys);
        for (int i = 0; i < zlistx_size (param_keys); i++) {
            self->parameter_names [i] = key;
            self->parameter_values [i] = NULL;
            key = (char *) zlistx_next (param_keys);
        }
    }
    if (self->token_type == NODE_TYPE_REGEX || self->token_type == NODE_TYPE_PARAM)
        self->regex = zrex_new (self->token);
    self->data = NULL;
    //  Initialize relationships
    self->parent = parent;
    if (self->parent) {
        zlistx_add_end (self->parent->children, self);
        //  Sort regexes to the end to avoid conlficts
        zlistx_sort (self->parent->children);
    }
    size_t parent_path_len = self->parent ? self->parent->path_len : 0;
    self->path_len = parent_path_len + strlen (self->token) + 1; // +1 path delimiter
    self->children = zlistx_new ();
    zlistx_set_comparator (self->children, s_ztrie_node_compare);

    return self;
}


//  Destroy the ztrie_node.

static void
s_ztrie_node_destroy (ztrie_node_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        ztrie_node_t *self = *self_p;

        //  Free class properties
        free (self->token);
        if (self->parameter_count > 0) {
            for (int i = 0; i < self->parameter_count; i++) {
                free (self->parameter_names [i]);
                if (self->parameter_values [i])
                    free (self->parameter_values [i]);
            }
            free (self->parameter_names);
            free (self->parameter_values);
        }
        if (self->token_type == NODE_TYPE_REGEX || self->token_type == NODE_TYPE_PARAM)
            zrex_destroy (&self->regex);
        zlistx_destroy (&self->children);
        if (self->data) {
            if (self->destroy_data_fn)
                (self->destroy_data_fn) (self->data);
            else {
                free (self->data);
            }
        }

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  Get the entire path of a node

static const char *
s_ztrie_node_path (ztrie_node_t *self)
{
    assert (self);
    char *path = (char *) zmalloc (self->path_len);
    path[self->path_len - 1] = '\0';

    ztrie_node_t *node = self;
    while (node->parent) {
        memcpy (path + node->parent->path_len, node->token, strlen (node->token));
        path[node->parent->path_len - 1] = '/';
        node = node->parent;
    }
    memcpy (path + 1, node->token, strlen (node->token));
    return path;
}

//  Update the value of a regex parameter at position.

static void
s_ztrie_node_update_param (ztrie_node_t *self, int pos, const char *value)
{
    assert (self);
    self->parameter_values [pos - 1] = strdup (value);
}


//  --------------------------------------------------------------------------
//  Create a new ztrie.

ztrie_t *
ztrie_new ()
{
    ztrie_t *self = (ztrie_t *) zmalloc (sizeof (ztrie_t));
    assert (self);

    //  Initialize properties
    self->root = s_ztrie_node_new (NULL, "", 0,  NULL, NODE_TYPE_STRING);
    self->match = NULL;
    self->params = zlistx_new ();

    return self;
}

//  Destroy all children of a node

static void
s_ztrie_destroy_children (ztrie_node_t *node)
{
    ztrie_node_t *child = (ztrie_node_t *) zlistx_first (node->children);
    while (child) {
        s_ztrie_destroy_children (child);
        s_ztrie_node_destroy (&child);
        child = (ztrie_node_t *) zlistx_next (node->children);
    }
}

//  --------------------------------------------------------------------------
//  Destroy the ztrie.

void
ztrie_destroy (ztrie_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        ztrie_t *self = *self_p;

        //  Free class properties
        s_ztrie_destroy_children (self->root);
        s_ztrie_node_destroy (&self->root);
        zlistx_destroy (&self->params);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  String compare token

static ztrie_node_t *
s_ztrie_compare_token (ztrie_node_t *parent, char *token, int len)
{
    ztrie_node_t *child = (ztrie_node_t *) zlistx_first (parent->children);
    while (child) {
        if (child->token_len == len &&
            strncmp (child->token, token, MIN_LEN (child->token_len, len)) == 0)
            return child;
        child = (ztrie_node_t *) zlistx_next (parent->children);
    }
    return NULL;
}

//  String compare token or evaluate regexes

static ztrie_node_t *
s_ztrie_matches_token (ztrie_node_t *parent, char *token, int len)
{
    unsigned int i;
    char firstbyte = *token;
    ztrie_node_t *child = (ztrie_node_t *) zlistx_first (parent->children);
    while (child) {
        if (child->token_type == NODE_TYPE_STRING) {
            if (firstbyte == *child->token  //  This achieves a small performance boost
                && child->token_len == len
                && strncmp (child->token, token, MIN_LEN (child->token_len, len)) == 0)
                    return child;
        }
        else {
            //  Need to copy token as zrex_matches expects '\0' terminated string
            char *token_term = strndup (token, len);
            if (zrex_matches (child->regex, token_term)) {
                if (child->token_type == NODE_TYPE_PARAM) {
                    //  One hit means no capturing group was defined
                    //  More than one hit indicates that at least on capturing group.
                    //  In this case only the values of the capturing groups are considered.
                    if (zrex_hits (child->regex) == 1)
                        s_ztrie_node_update_param (child, 1, zrex_hit (child->regex, 0));
                    else
                    if (zrex_hits (child->regex) > 1)
                        for (i = 1; i < zrex_hits (child->regex); i++)
                            s_ztrie_node_update_param (child, i, zrex_hit (child->regex, i));
                }
                free (token_term);
                return child;
            }
            free (token_term);
        }
        child = (ztrie_node_t *) zlistx_next (parent->children);
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Parses an path bytewise trying to find matches for path tokens. Depending
//  on the mode there are different behaviors if no match could be found:
//     MODE_INSERT: creates new vertex and attaches it the common prefix for
//                  the given path, repeat for the remaining path tokens.
//                  returns the vertex that has been created last.
//     MODE_MATCH: returns NULL as the lookup failed.
//     MODE_PREFIX: returns the common prefix for the given path.

static ztrie_node_t *
s_ztrie_parse_path (ztrie_t *self, char *path, int mode)
{
    int state = 0;
    char *needle, *beginToken = NULL, *beginRegex = NULL;
    ztrie_node_t *parent = self->root;
    if (zlistx_size (self->params) > 0)
        zlistx_purge (self->params);

    int len = strlen (path);
    needle = path;
    char *needle_stop = needle + len;
    while (needle < needle_stop + 1) {
        if (*needle == '/' || needle == needle_stop) {
            //  token starts with '/' ignore everything that comes before
            if (state == 0) {
                beginToken = needle + 1;
                state++;
                if (mode == MODE_INSERT)
                    state++;
            }
            //  token ends with '/'
            else
            if (state < 3) {
                int matchType = zlistx_size (self->params) > 0 ? NODE_TYPE_PARAM :
                                    beginRegex ? NODE_TYPE_REGEX : NODE_TYPE_STRING;
                char *matchToken = beginRegex ? beginRegex : beginToken;
                int matchTokenLen = needle - matchToken - (beginRegex ? 1 : 0);
                ztrie_node_t *match = NULL;
                //  In insert mode only do a string comparison
                if (mode == MODE_INSERT)
                    match = s_ztrie_compare_token (parent, matchToken, matchTokenLen);
                //  Otherwise evaluate regexes
                else
                    match = s_ztrie_matches_token (parent, matchToken, matchTokenLen);
                //  Mismatch behavior depends on mode
                if (!match) {
                    //  Append to common prefix
                    if (mode == MODE_INSERT) {
                        match = s_ztrie_node_new (parent, matchToken, matchTokenLen, self->params, matchType);
                    }
                    //  Commen prefix found
                    if (mode == MODE_PREFIX) {
                        break;
                    }
                    //  No match for path found
                    if (mode == MODE_MATCH) {
                        parent = NULL;
                        break;
                    }
                }
                //  If a match has been found it becomes the parent for next path token
                if (match);
                    parent = match;
                //  Cleanup for next token
                beginRegex = NULL;
                if (zlistx_size (self->params) > 0)
                    zlistx_purge (self->params);
                //  Token end equals token begin
                beginToken = needle + 1;
            }
        }
        else
        //  regex starts with '{'
        if (state == 2 && *needle == '{') {
            beginRegex = needle + 1;
            state++;
        }
        else
        //  in the middle of the regex
        if (state == 3 && (*needle == ':')) {
            zlistx_add_end (self->params, strndup (beginRegex, needle - beginRegex));
            beginRegex = needle + 1;

        }
        else
        // regex ends with {
        if (state == 3 && *needle == '}') {
            state--;
        }
        needle++;
    }

    //  Cleanup
    //  In matching mode the discovered node must be an endpoint
    if (parent && mode == MODE_MATCH && !parent->endpoint)
        return NULL;

    return parent;
}


//  --------------------------------------------------------------------------
//  Inserts a new route into the tree and attaches the data. Returns -1
//  if the route already exists, otherwise 0. This method takes ownership of
//  the provided data.

int
ztrie_insert_route (ztrie_t *self, char *path, void *data, ztrie_destroy_data_fn *destroy_data_fn)
{
    assert (self);
    /*printf ("Path: %s\n", path);*/
    ztrie_node_t *last = s_ztrie_parse_path (self, path, MODE_INSERT);
    if (!last->endpoint) {
        last->endpoint = true;
        last->data = data;
        last->destroy_data_fn = destroy_data_fn;
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Returns true if the path matches a route in the tree, otherwise false.

bool
ztrie_matches (ztrie_t *self, char *path)
{
    assert (self);
    self->match = s_ztrie_parse_path (self, path, MODE_MATCH);
    return self->match ? true : false;
}


//  --------------------------------------------------------------------------
//  Returns the data of a matched route from last ztrie_matches. If the path
//  did not match, returns NULL. Do not delete the data as it's owned by
//  ztrie.

void *
ztrie_hit_data (ztrie_t *self)
{
    assert (self);
    if (self->match)
        return self->match->data;
    return NULL;
}


//  --------------------------------------------------------------------------
//  Returns the parameters of a matched route with named regexes from last
//  ztrie_matches. If the path did not match or the route did not contain any
//  named regexes, returns NULL. The caller is responseable for destroy the map.

zhashx_t *
ztrie_hit_parameters (ztrie_t *self)
{
    assert (self);
    if (self->match) {
        zhashx_t *route_parameters = zhashx_new ();
        ztrie_node_t *node = self->match;
        while (node) {
            for (int i = 0; i < node->parameter_count; i++)
                zhashx_insert (route_parameters,
                              node->parameter_names [i],
                              (void *) node->parameter_values [i]);
            node = node->parent;
        }
        return route_parameters;
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Print properties of the ztrie object.
//
static void
s_ztrie_print_tree_line (ztrie_node_t *self, bool isEOL)
{
    if (self->parent) {
        s_ztrie_print_tree_line (self->parent, false);
        if (zlistx_tail (self->parent->children) == self) {
            if (isEOL)
                printf ("\u2514\u2500\u2500 ");
            else
                printf ("    ");
        }
        else {
            if (isEOL)
                printf ("\u251C\u2500\u2500 ");
            else
                printf ("\u2502   ");
        }
        if (isEOL) {
            char *isEndpoint = self->endpoint ? "true" : "false";
            printf ("%s (params: %zu, endpoint: %s, type: %d)\n", self->token,
                                                                 self->parameter_count,
                                                                 isEndpoint,
                                                                 self->token_type);
        }
    }
}

static void
s_ztrie_print_tree (ztrie_node_t *self)
{
    //  Print tree like structure
    s_ztrie_print_tree_line (self, true);
    ztrie_node_t *child = (ztrie_node_t *) zlistx_first (self->children);
    while (child) {
        s_ztrie_print_tree (child);
        child = (ztrie_node_t *) zlistx_next (self->children);
    }
}

void
ztrie_print (ztrie_t *self)
{
    assert (self);
    s_ztrie_print_tree (self->root);
}

//  --------------------------------------------------------------------------
//  Self test of this class.

void
ztrie_test (bool verbose)
{
    printf (" * ztrie: ");

    //  @selftest
    //  Simple create/destroy test
    ztrie_t *self = ztrie_new ();
    assert (self);

    int ret = 0;

    ret = ztrie_insert_route (self, "/{[^/]+}", NULL, NULL);
    ztrie_print (self);
    assert (ret == 0);

    ret = ztrie_insert_route (self, "/foo/bar", NULL, NULL);
    ztrie_print (self);
    assert (ret == 0);

    ret = ztrie_insert_route (self, "/foo/foo", NULL, NULL);
    ztrie_print (self);
    assert (ret == 0);

    ret = ztrie_insert_route (self, "/foo", NULL, NULL);
    ztrie_print (self);
    assert (ret == 0);

    ret = ztrie_insert_route (self, "/foo", NULL, NULL);
    ztrie_print (self);
    assert (ret == -1);

    ret = ztrie_insert_route (self, "/foo/{[^/]+}", NULL, NULL);
    ztrie_print (self);
    assert (ret == 0);

    char *data = (char *) malloc (80);
    sprintf (data, "%s", "Hello World!");
    ret = ztrie_insert_route (self, "/baz/{name:[^/]+}/{id:(\\d+)}/{street:nr:(\\a+)(\\d+)}", data, NULL);
    ztrie_print (self);
    assert (ret == 0);

    //  Test matches()
    bool hasMatch = false;

    hasMatch = ztrie_matches (self, "/bar/foo");
    assert (!hasMatch);

    hasMatch = ztrie_matches (self, "/foo/bar");
    assert (hasMatch);

    hasMatch = ztrie_matches (self, "/baz/blub");
    assert (!hasMatch);

    hasMatch = ztrie_matches (self, "/baz/blub/11/abc23");
    assert (hasMatch);
    char *match_data = (char *) ztrie_hit_data (self);
    assert (streq ("Hello World!", match_data));
    zhashx_t *parameters = ztrie_hit_parameters (self);
    assert (zhashx_size (parameters) == 4);
    assert (streq ("blub", zhashx_lookup (parameters, "name")));
    assert (streq ("11", zhashx_lookup (parameters, "id")));
    assert (streq ("abc", zhashx_lookup (parameters, "street")));
    assert (streq ("23", zhashx_lookup (parameters, "nr")));
    zhashx_destroy (&parameters);

    s_ztrie_node_path (self->root);

    ztrie_destroy (&self);
    //  @end

    printf ("OK\n");
}
