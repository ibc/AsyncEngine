#!/usr/bin/env ruby

$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../../", "lib"))
require "asyncengine"


  #t2 = Thread.new { loop do puts "\nTHREAD 2" ; AE.debug ; sleep 1 ; end }

loop do
  t = Thread.new do
    AE.run do
      AE::PeriodicTimer.new(0.1) {printf "."}
    end
  end

  sleep 0.2

  t.kill
end

  #t2.join

  

