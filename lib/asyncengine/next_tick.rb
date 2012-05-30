module AsyncEngine

  def self.next_tick pr=nil, &bl
    ensure_ready_for_handles()

    @_next_ticks << (pr || bl)
    _c_next_tick
  end

  def self.execute_next_ticks
    ticks, @_next_ticks = @_next_ticks, []
    ticks.each do |cb|
      begin
        cb.call
      rescue StandardError => e
        AsyncEngine.send :handle_exception, e
      end
    end
  end

  class << self
    private :_c_next_tick
    private :execute_next_ticks
  end

end
