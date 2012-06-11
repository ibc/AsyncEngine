module AsyncEngine

  # TODO: si peta un next_tick, los siguientes que había en el array se pierden !
  def self.execute_next_ticks
    ticks, @_next_ticks = @_next_ticks, []
    ticks.each do |cb|
      begin
        cb.call
      rescue StandardError => e
        AsyncEngine.send :handle_error, e
      end
    end
  end

  class << self
    private :execute_next_ticks
  end

end
