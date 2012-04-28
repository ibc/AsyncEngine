module AsyncEngine

  def self.add_timer delay, callback=nil, &block
    AsyncEngine._c_add_timer((delay*1000).to_i, nil, callback || block)
    true
  end

  def self.add_periodic_timer interval, delay=nil, callback=nil, &block
    delay ||= interval
    AsyncEngine._c_add_timer((delay*1000).to_i, (interval*1000).to_i, callback || block)
    true
  end


  class Timer
    def initialize delay, callback=nil, &block
      @_timer_data = AsyncEngine._c_add_timer((delay*1000).to_i, nil, callback || block)
    end
  end

  class PeriodicTimer < Timer
    def initialize interval, delay=nil, callback=nil, &block
      delay ||= interval
      @_timer_data = AsyncEngine._c_add_timer((delay*1000).to_i, (interval*1000).to_i, callback || block)
    end
  end

end