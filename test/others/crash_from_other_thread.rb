#!/usr/bin/env ruby1.9.1

$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../../", "lib"))

require "asyncengine"

def nextTick v
  printf v
  t = AE::Timer.new(0.1) {}
  AE.add_timer(0.05) { t.cancel }
  AE.next_tick { nextTick v }
end

10.times do
  Thread.new do
    nextTick "t"
  end
end

AE.run do
  nextTick "a"
end