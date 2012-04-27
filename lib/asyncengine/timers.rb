module AsyncEngine

  def self.add_timer timeout, callback=nil, &block
    AsyncEngine._c_add_timer((timeout*1000).to_i, nil, callback || block)
    true
  end

  def self.add_periodic_timer interval, callback=nil, &block
    AsyncEngine._c_add_timer(nil, (interval*1000).to_i, callback || block)
    true
  end

  class Timer
    def initialize timeout, callback=nil, &block
      @handle = AsyncEngine._c_add_timer((timeout*1000).to_i, nil, callback || block)
    end

    def stop
      AsyncEngine._c_stop_timer @handle
    end
  end

  class PeriodicTimer < Timer
    def initialize interval, callback=nil, &block
      @handle = AsyncEngine._c_add_timer(nil, (interval*1000).to_i, callback || block)
    end
  end
end