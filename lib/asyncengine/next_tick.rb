module AsyncEngine

  def self.next_tick block1=nil, &block2
    @_next_ticks << (block1 || block2)
    _c_next_tick
  end

  def self.execute_next_ticks
    ticks, @_next_ticks = @_next_ticks, []
    ticks.each do |cb|
      begin
        cb.call
      rescue => e
        AsyncEngine.send :handle_error, e
      end
    end
  end

  class << self
    private :_c_next_tick
    private :execute_next_ticks
  end

end
