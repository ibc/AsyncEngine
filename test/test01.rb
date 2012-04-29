#!/usr/bin/env ruby1.9.1

$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../", "lib"))

require "asyncengine"



#trap(:INT)  { puts "*** INT trapped => ignore" }
#trap(:INT)  { puts "*** INT trapped => exit" ; exit }


puts "PID = #{$$}"

at_exit { puts "NOTICE: exiting..." }


#Thread.new { loop { puts "---t1---" ; sleep 0.001 } }
#Thread.new { loop { puts "---t2---" ; sleep 0.001 } }

#AE.add_periodic_timer(0.001) { puts "***AE timer 1***" }
#AE.add_periodic_timer(0.001) { puts "***AE timer 2***" }


if true and false
  t1 = AE::PeriodicTimer.new(1,0) { puts "--- t1 periodic timer should be stopped after some seconds !!! ---" }
  puts t1.inspect
  AE.add_timer(4) do
    t2 = AE::Timer.new(2) { puts "--- t2 single timer should NOT be stopped !!! ---" }
    puts t2.inspect
    puts "--- canceling t1 ---"
    t1.cancel
    AE.add_timer(1) { puts "exiting..." ; exit }
  end
end


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


if true and false
  cancel = false
  100.times { AE.add_periodic_timer(0.001) { a = AE::Timer.new(1){  } ; a.cancel if cancel } }
end


#AE.add_timer(0.5) { puts "BUMMMP" } ; AE.add_timer(1) { puts "LAST" }


#AE.add_timer(2) { raise "raising an exception" }


if true #and false
  $interval = 0.5
  pt1 = AE::PeriodicTimer.new($interval) do
    if $interval == 8
      puts "--- pt1: interval is 8, stopping timer"
      pt1.stop
    else
      puts "--- pt1: interval=#{$interval} , setting next interval in #{$interval * 2}"
      pt1.interval = ($interval *= 2)
    end
  end
end


AE.start do
  AE.add_periodic_timer(2) do
    if AsyncEngine.num_handles > 1
      puts "DBG: AE.num_handles = #{AsyncEngine.num_handles}"
    else
      puts "NOTICE: no more handles, exiting"
      exit true
    end
  end
end


puts "END"