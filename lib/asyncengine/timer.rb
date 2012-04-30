module AsyncEngine

  def self.add_timer delay, callback=nil, &block
    _c_add_timer((delay*1000).to_i, nil, callback || block, nil)
    true
  end

  def self.add_periodic_timer interval, delay=nil, callback=nil, &block
    if delay
      @_c_data = _c_add_timer((delay*1000).to_i, (interval*1000).to_i, callback || block, nil)
    else
      interval = (interval*1000).to_i
      @_c_data = _c_add_timer(interval, interval, callback || block, nil)
    end
    true
  end

  class << self
    private :_c_add_timer
  end


  class Timer
    def initialize delay, callback=nil, &block
      @_c_data = AsyncEngine.send(:_c_add_timer, (delay*1000).to_i, nil, callback || block, self)
    end
  end


  class PeriodicTimer < Timer
    def initialize interval, delay=nil, callback=nil, &block
      if delay
        @_c_data = AsyncEngine.send(:_c_add_timer, (delay*1000).to_i, (interval*1000).to_i, callback || block, self)
      else
        interval = (interval*1000).to_i
        @_c_data = AsyncEngine.send(:_c_add_timer, interval, interval, callback || block, self)
      end
    end

    def set_interval interval
      _c_set_interval (interval*1000).to_i
    end
    alias :interval= :set_interval
  end

end