#!/usr/bin/env ruby

$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../../", "lib"))

require "asyncengine"
require "eventmachine-le"
require "benchmark"


TIMER_TIMES = 3000
NEXT_TICK_TIMES = 600000


puts "1) Periodic timer with interval 0.001 seconds fires #{TIMER_TIMES} times:"
puts

printf "- EM::PeriodicTimer: "
puts Benchmark.realtime {
  $i = 0
  EM.run do
    #t = EM::PeriodicTimer.new(0.001) do
    EM.add_periodic_timer(0.001) do
      $i += 1
      EM.stop  if $i == TIMER_TIMES
    end
  end
}.to_s + " seconds"

printf "- AE::PeriodicTimer: "
puts Benchmark.realtime {
  $i = 0
  AE.run do
    AE::PeriodicTimer.new(0.001) do
      $i += 1
      AE.stop  if $i == TIMER_TIMES
    end
  end
}.to_s + " seconds"


puts
puts "2) Next tick gives #{NEXT_TICK_TIMES} ticks:"
puts

printf "- EM::next_tick: "
puts Benchmark.realtime {
  $i = 0
  EM.run do
    (NEXT_TICK_TIMES).times do
      EM.next_tick do
        $i += 1
        EM.stop  if $i == NEXT_TICK_TIMES
      end
    end
  end
}.to_s + " seconds"

printf "- AE::next_tick: "
puts Benchmark.realtime {
  $i = 0
  AE.run do
    (NEXT_TICK_TIMES).times do
      AE.next_tick do
        $i+=1
        AE.stop  if $i == NEXT_TICK_TIMES
      end
    end
  end
}.to_s + " seconds"
