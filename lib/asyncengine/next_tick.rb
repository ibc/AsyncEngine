module AsyncEngine

  def self.next_tick callback=nil, &block
    @next_ticks << (callback || block)
    _c_next_tick
  end

  def self.execute_next_ticks
    @next_ticks.each do |cb|
      cb.call
    end
    @next_ticks.clear
  end

  class << self
    private :_c_next_tick
    private :execute_next_ticks
  end

end
