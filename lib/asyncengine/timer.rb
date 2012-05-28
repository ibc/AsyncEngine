module AsyncEngine

  def self.add_timer delay, pr=nil, &bl
    Timer.new delay, pr || bl
  end

  def self.add_periodic_timer interval, delay=nil, pr=nil, &bl
    PeriodicTimer.new interval, delay, pr || bl
  end


  class Timer < Handle
    def initialize delay, pr=nil, &bl
      AsyncEngine.send :ensure_ready_for_handles

      uv_handle_init (delay = (delay*1000).to_i), nil, pr || bl
    end

    def restart delay=nil
      delay = (delay*1000).to_i  if delay
      _c_restart delay, nil
    end

    alias orig_to_s to_s
    def to_s
      if alive?
        "#{orig_to_s} (delay: #{delay().to_f/1000})"
      else
        "#{orig_to_s} (not alive)"
      end
    end
    alias :inspect :to_s
  end


  class PeriodicTimer < Timer
    def initialize interval, delay=nil, pr=nil, &bl
      AsyncEngine.send :ensure_ready_for_handles

      interval = (interval*1000).to_i
      delay = ( delay ? (delay*1000).to_i : interval )
      uv_handle_init delay, interval, pr || bl
    end

    def restart interval=nil, delay=nil
      interval = (interval*1000).to_i  if interval
      delay = ( delay ? (delay*1000).to_i : interval )
      _c_restart delay, interval
    end

    def to_s
      if alive?
        "#{orig_to_s} (delay: #{delay().to_f/1000}}, interval: #{interval().to_f/1000})"
      else
        "#{orig_to_s} (not alive)"
      end
    end
    alias :inspect :to_s
  end

end