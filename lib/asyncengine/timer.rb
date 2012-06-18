module AsyncEngine

  def self.add_timer *params, &bl
    Timer.new *params, &bl
  end

  def self.add_periodic_timer *params, &bl
    PeriodicTimer.new *params, &bl
  end


  class Timer < Handle
    alias orig_to_s to_s
    def to_s
      if alive?
        "#{orig_to_s} (delay: #{delay()})"
      else
        "#{orig_to_s} (not alive)"
      end
    end
    alias :inspect :to_s
  end


  class PeriodicTimer < Timer
    def to_s
      if alive?
        "#{orig_to_s} (delay: #{delay()}, interval: #{interval()})"
      else
        "#{orig_to_s} (not alive)"
      end
    end
    alias :inspect :to_s
  end

end