#!/usr/bin/env ruby
arr    = ["foo", "bar", "baz", "qux", "quux", "corge", "grault", "garply"]
paths  = arr.permutation(6).map { |a| "/#{a.join '/'}" }
paths.each_with_index do |path, index|
    # puts "    ztrie_insert_route (n, #{path}, NULL, NULL);"
    puts "    zhash_insert (n, \"#{path}\", NULL);"
end
