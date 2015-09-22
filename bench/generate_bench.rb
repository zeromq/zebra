#!/usr/bin/env ruby
arr    = ["foo", "bar", "baz", "qux", "quux", "corge", "grault", "garply"]
paths  = arr.permutation(3).map { |a| "/#{a.join '/'}" }
paths.each_with_index do |path, index|
    # puts "    char str#{index}[80] = \"#{path}\";"
    # puts "    ztrie_insert_route (n, str#{index}, NULL, NULL);"
    puts "    zhash_insert (n, \"#{path}\", NULL);"
end
