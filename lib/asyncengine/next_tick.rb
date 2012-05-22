module AsyncEngine

  def self.next_tick pr=nil, &bl
    @_next_ticks << (pr || bl)
    _c_next_tick
  end

  def self.execute_next_ticks
    ticks, @_next_ticks = @_next_ticks, []
    ticks.each do |cb|
      begin
        cb.call
      # TODO: lalala
      rescue StandardError, LoadError => e
        AsyncEngine.send :handle_exception, e
      end
    end
  end

  class << self
    private :_c_next_tick
    private :execute_next_ticks
  end

end
