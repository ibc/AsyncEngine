#!/usr/bin/env ruby

require "eventmachine"
$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../", "lib"))
require "asyncengine"


Thread.abort_on_exception = true


thread_ticks = 0
ae_timer_ticks = 0
em_timer_ticks = 0
time_start = Time.now

at_exit do
  puts "\n\nINFO: exiting...\n- interval = #{Time.now - time_start} seconds\n- thread_ticks = #{thread_ticks}\n- em_timer_ticks = #{em_timer_ticks}\n- ae_timer_ticks = #{ae_timer_ticks}\n\n"

  exit!
end

# Ruby thread doing a loop.
t1 = Thread.new { loop { printf "T" ; sleep 0.001 ; thread_ticks+=1 } }

# EventMachine with periodic timer.
t2 = Thread.new { EM.run { EM.add_periodic_timer(0.001) { printf "E" ; em_timer_ticks+=1} } }

# AsyncEngine with periodic timer.
t3 = Thread.new { AE.run { AE.add_periodic_timer(0.001) { printf "A" ; ae_timer_ticks+=1} } }

t1.join rescue nil
t2.join rescue nil
t3.join rescue nil
