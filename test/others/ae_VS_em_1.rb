$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../../", "lib"))

require "asyncengine"
require "eventmachine-le"
require "benchmark"


TIMES = 3000


puts "PID = #{$$}"
sleep 0.5
puts

puts
printf "1) AE::PeriodicTimer: "
puts Benchmark.realtime {
  $i = 0
  AE.run do
    t = AE::PeriodicTimer.new(0.001) do
      #printf(".")
      $i += 1
      t.cancel  if $i == TIMES
    end
  end
}

puts
printf "2) EM::PeriodicTimer: "
puts Benchmark.realtime {
  $i = 0
  EM.run do
    t = EM::PeriodicTimer.new(0.001) do
      #printf(".")
      $i += 1
      EM.stop  if $i == TIMES
    end
  end
}

puts
printf "3) AE::next_tick: "
puts Benchmark.realtime {
  $i = 0
  AE.run do
    (TIMES*100).times do
      AE.next_tick do
        #printf(".")
        $i+=1
        printf()  if $i == TIMES
      end
    end
  end
}

puts
printf "4) EM::next_tick: "
puts Benchmark.realtime {
  $i = 0
  EM.run do
    (TIMES*100).times do
      EM.next_tick do
        #printf(".")
        $i += 1
        EM.stop  if $i == TIMES
      end
    end
  end
}
