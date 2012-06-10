module AsyncEngine

  def self.execute_call_from_other_thread_procs
    # NOTE: This action is atomic, so we don't need a Mutex.
    procs, @_call_from_other_thread_procs = @_call_from_other_thread_procs, []

    procs.each do |pr|
      begin
        pr.call
      rescue StandardError => e
        AsyncEngine.send :handle_exception, e
      end
    end
  end


  class << self
    private :execute_call_from_other_thread_procs
  end

end