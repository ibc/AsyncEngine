module AsyncEngine

  def self.add_timer delay, block1=nil, &block2
    _c_add_timer((delay*1000).to_i, nil, block1 || block2, nil)
    true
  end

  def self.add_periodic_timer interval, delay=nil, block1=nil, &block2
    if delay
      _c_add_timer((delay*1000).to_i, (interval*1000).to_i, block1 || block2, nil)
    else
      interval = (interval*1000).to_i
      _c_add_timer(interval, interval, block1 || block2, nil)
    end
    true
  end

  class << self
    private :_c_add_timer
  end


  class Timer
    def initialize delay, block1=nil, &block2
      @_cdata = AsyncEngine.send(:_c_add_timer, (delay*1000).to_i, nil, block1 || block2, self)
    end

    def active?
      @_handle_terminated ? false : true
    end
  end


  class PeriodicTimer < Timer
    def initialize interval, delay=nil, block1=nil, &block2
      if delay
        @_cdata = AsyncEngine.send(:_c_add_timer, (delay*1000).to_i, (interval*1000).to_i, block1 || block2, self)
      else
        interval = (interval*1000).to_i
        @_cdata = AsyncEngine.send(:_c_add_timer, interval, interval, block1 || block2, self)
      end
    end

    def set_interval interval
      _c_set_interval (interval*1000).to_i
    end
    alias :interval= :set_interval
  end

end