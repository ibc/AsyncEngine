#!/usr/bin/env ruby

require "eventmachine"
$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../", "lib"))
require "asyncengine"


Thread.abort_on_exception = true


$ae_timer_ticks = 0
$em_timer_ticks = 0
$time_start = Time.now

at_exit do
  puts "\n\nINFO: exiting...\n- interval = #{Time.now - $time_start} seconds\n- em_timer_ticks = #{$em_timer_ticks}\n- ae_timer_ticks = #{$ae_timer_ticks}\n\n"

  exit!
end

def add_em_tick
  printf "E"
  $em_timer_ticks+=1
  EM.next_tick { add_em_tick }
end

def add_ae_tick
  printf "A"
  $ae_timer_ticks+=1
  AE.next_tick { add_ae_tick }
end



# EventMachine with next_tick.
t1 = Thread.new { EM.run { EM.next_tick { add_em_tick } } }

# AsyncEngine with next_tick.
t2 = Thread.new { AE.run { AE.next_tick { add_ae_tick } } }

t1.join rescue nil
t2.join rescue nil
