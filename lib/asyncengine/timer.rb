module AsyncEngine

  def self.add_timer delay, block1=nil, &block2
    Timer.new delay, block1 || block2
  end

  def self.add_periodic_timer interval, delay=nil, block1=nil, &block2
    PeriodicTimer.new interval, delay, block1 || block2
  end


  class Timer
    def initialize delay, block1=nil, &block2
      uv_init delay = (delay*1000).to_i, nil, block1 || block2
    end

    def restart delay=nil
      delay = (delay*1000).to_i  if delay
      _c_restart delay, nil
    end

    alias orig_to_s to_s
    def to_s
      "#{self.orig_to_s} (delay: #{delay})"
    end
    alias :inspect :to_s
  end


  class PeriodicTimer < Timer
    def initialize interval, delay=nil, block1=nil, &block2
      interval = (interval*1000).to_i
      delay = ( delay ? (delay*1000).to_i : interval )
      uv_init delay, interval, block1 || block2
    end

    def restart interval=nil, delay=nil
      interval = (interval*1000).to_i  if interval
      delay = ( delay ? (delay*1000).to_i : interval )
      _c_restart delay, interval
    end

    def to_s
      "#{self.orig_to_s} (delay: #{delay}, interval: #{interval})"
    end
    alias :inspect :to_s
  end

end