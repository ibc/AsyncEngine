#!/usr/bin/env ruby

require "eventmachine"
$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../../", "lib"))
require "asyncengine"


Thread.abort_on_exception = true


thread_ticks = 0
ae_timer_ticks = 0
em_timer_ticks = 0
time_start = Time.now

at_exit do
  puts "\n\nINFO: exiting..."
  puts "- interval = #{interval = Time.now - time_start} seconds"
  puts "- thread_ticks = #{thread_ticks} (#{(thread_ticks / (interval*10)).to_i}%)"
  puts "- em_timer_ticks = #{em_timer_ticks} (#{(em_timer_ticks / (interval*10)).to_i}%)"
  puts "- ae_timer_ticks = #{ae_timer_ticks} (#{(ae_timer_ticks / (interval*10)).to_i}%)"
  puts
  exit!
end

# Ruby thread doing a loop.
t1 = Thread.new { loop { printf "T" ; sleep 0.001 ; thread_ticks+=1 } }

# EventMachine with periodic timer.
t2 = Thread.new { EM.run { EM.add_periodic_timer(0.001) { printf "E" ; em_timer_ticks+=1} } }
#t2 = Thread.new { EM.run { EM::PeriodicTimer.new(0.001) { printf "E" ; em_timer_ticks+=1} } }

# AsyncEngine with periodic timer.
t3 = Thread.new { AE.run { AE.add_periodic_timer(0.001) { printf "A" ; ae_timer_ticks+=1} } }
#t3 = Thread.new { AE.run { AE::PeriodicTimer.new(0.001) { printf "A" ; ae_timer_ticks+=1} } }

t1.join rescue nil
t2.join rescue nil
t3.join rescue nil
