#!/usr/bin/env ruby

$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../../", "lib"))
require "asyncengine"



puts $$

Thread.new { sleep 1 ; Process.kill("INT", $$) }

loop do AE.run { AE.next_tick { AE.next_tick { } } } end

puts "FIN :)"
