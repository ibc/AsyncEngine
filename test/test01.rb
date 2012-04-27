#!/usr/bin/env ruby

$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../", "lib"))

require "asyncengine"

Thread.new { loop { puts "." ; sleep 1 } }

exit_signals = ["sigTERM", :quit, :INt]
exit_signals.each do |signal|
  AE.on_signal signal do
    $stderr.puts "Ruby #{signal} signal received, exiting..."
    exit 0
  end
end

AE.on_signal(:USR2) { puts "*** C USR2 ignorado" }
#trap(:USR2) { puts "*** Ruby USR1 ignorado" }


AE._c_add_handle "AAA"
AE._c_add_handle "BBB"
AE._c_add_handle 123

puts (AE.instance_variable_get :@handles).inspect

AE.start do
  puts "YA"
end

puts "FIN"