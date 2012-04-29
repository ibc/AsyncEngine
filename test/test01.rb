#!/usr/bin/env ruby

$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../", "lib"))

require "asyncengine"



#trap(:TERM)  { puts "*** TERM trapped => ignore" }
#trap(:INT)   { puts "*** INT trapped => exit" ; exit }


puts "PID = #{$$}"

t = Thread.current
#Thread.new { loop { printf "." ; sleep 0.005 } }
Thread.new { sleep 5; puts "YA" ; t.kill }


# AE.add_timer(0.1) { puts "1 TIMER !!!" }
# AE.add_timer(0.12) { puts "3 TIMER !!!" }
# 

# t1 = AE::Timer.new(2) { puts "--- t1 should be stopped !!! ---" }
# puts t1.inspect
#  
# AE.add_timer(1) do
#   t2 = AE::Timer.new(2) { puts "--- t2 should be stopped !!! ---" }
#   puts t2.inspect
#   t1.cancel
#   t2.cancel
# end


#AE.add_timer(4) { puts "--- TIMER 4 seconds !!!" }
AE.add_periodic_timer(2) { puts "--- AE timer !!!" }


if true and false
  class KK
    def initialize a,b
      @a = a
      @b = b
    end
  end

  require "securerandom"
  $count = 0
  AE.add_periodic_timer(0.5) do
    AE.add_timer(0) do
      kk = KK.new(SecureRandom.hex, (rand*1000).to_i)
      puts "- #{$count += 1}: #{kk.inspect}"
    end
  end
end


#1000.times { AE.add_timer(0) { "- #{$count+=1}" } }


# puts "*** AE.handles: #{(AE.instance_variable_get :@handles).inspect}"
# sleep 0.4

AE.start do
  #AE.add_timer(1) { puts "*** AE.handles: #{(AE.instance_variable_get :@handles).inspect}" }
end

puts "FIN"