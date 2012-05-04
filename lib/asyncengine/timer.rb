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
      @_delay = (delay*1000).to_i
      @_block = block1 || block2
      @_cdata = AsyncEngine.send(:_c_add_timer, @_delay, nil, @_block, self)
    end

    def active?
      @_handle_terminated ? false : true
    end

    # TODO: Doc.
    # If no delay and block are given, it reuses the previous ones.
    def restart delay=nil, block1=nil, &block2
      @_delay = (delay*1000).to_i  if delay
      @_block = block1 || block2 || @_block
      cancel
      @_handle_terminated = nil
      @_cdata = AsyncEngine.send(:_c_add_timer, @_delay, nil, @_block, self)
    end
  end


  class PeriodicTimer < Timer
    def initialize interval, delay=nil, block1=nil, &block2
      @_interval = (interval*1000).to_i
      @_delay = ( delay ? (delay*1000).to_i : @_interval )
      @_block = block1 || block2
      @_cdata = AsyncEngine.send(:_c_add_timer, @_delay, @_interval, @_block, self)
    end

    # TODO: Doc.
    # If no interval and block are given, it reuses the previous ones.
    # But if delay is not given, then it copies it from the new interval (if given).
    def restart interval=nil, delay=nil, block1=nil, &block2
      @_interval = (interval*1000).to_i  if interval
      @_delay = ( delay ? (delay*1000).to_i : @_interval )
      @_block = block1 || block2 || @_block
      cancel
      @_handle_terminated = nil
      @_cdata = AsyncEngine.send(:_c_add_timer, @_delay, @_interval, @_block, self)
    end

    # TODO: Doc.
    # Changes the interval in which the periodic timer fires.
    # If the timer was stopped this method takes no effect (and returns false).
    def set_interval interval
      _c_set_interval (interval*1000).to_i
    end
    alias :interval= :set_interval
  end

end