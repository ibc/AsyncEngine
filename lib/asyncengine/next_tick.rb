module AsyncEngine

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
