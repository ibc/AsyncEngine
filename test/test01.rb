#!/usr/bin/env ruby

$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../", "lib"))

require "asyncengine"


# Thread.new { loop { puts "." ; sleep 0.001 } }

exit_signals = ["sigTERM", :quit, :INt]
exit_signals.each do |signal|
  AE.on_signal signal do
    $stderr.puts "Ruby #{signal} signal received, exiting..."
    exit 0
  end
end

#AE.on_signal(:USR2) { puts "*** C USR2 ignorado" }
#trap(:USR2) { puts "*** Ruby USR1 ignorado" }


# AE._c_add_handle "AAA"
# AE._c_add_handle "BBB"
# AE._c_add_handle 123
# 
#puts (AE.instance_variable_get :@handles).inspect

# AE.add_timer(0.1) { puts "1 TIMER !!!" }
# AE.add_timer(0.12) { puts "3 TIMER !!!" }
# 
# t1 = AE::Timer.new(2) { puts "--- t1 should be stopped !!! ---" }
# puts t1.inspect
# #t2 = AE::Timer.new(2) { puts "--- t2 should be stopped !!! ---" }
# #puts t2.inspect
# 
# AE.add_timer(1) do
#   t2 = AE::Timer.new(2) { puts "--- t2 should be stopped !!! ---" }
#   puts t2.inspect
#   t1.cancel
# end

#AE.add_periodic_timer(0.001,0) { puts "---" }


class KK
  def initialize a,b
    @a = a
    @b = b
  end
end

require "securerandom"
  

$count = 0
AE.add_periodic_timer(0.001) do
  AE.add_timer(0) do
    kk = KK.new(SecureRandom.hex, (rand*1000).to_i)
    puts "- #{$count += 1}: #{kk.inspect}"
  end
end


#1000.times { AE.add_timer(0) { "- #{$count+=1}" } }


# puts "*** AE.handles: #{(AE.instance_variable_get :@handles).inspect}"
# sleep 0.4

AE.start do
  #AE.add_timer(1) { puts "*** AE.handles: #{(AE.instance_variable_get :@handles).inspect}" }
end

puts "FIN"