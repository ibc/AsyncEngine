module AsyncEngine

  def self.ensure_released
    raise AE::Error, "still running"  if running?()
    raise AE::Error, "num_uv_active_handles = #{num_uv_active_handles()} (> 1)"  unless num_uv_active_handles() == 0
    raise AE::Error, "num_uv_active_reqs = #{num_uv_active_reqs()} (> 1)"  unless num_uv_active_reqs() == 0
    raise AE::Error, "@_handles not empty"  unless @_handles.empty?
    raise AE::Error, "@_procs not empty"  unless @_procs.empty?
    raise AE::Error, "@_next_tick_procs not empty"  unless @_next_tick_procs.empty?
    raise AE::Error, "@_call_from_other_thread_procs not empty"  unless @_call_from_other_thread_procs.empty?
    @_call_from_other_thread_procs
  end

  def self.debug
    puts "\nDBG: AsyncEngine debug:"
    puts "- AE.running: #{running?}"
    puts "- UV active handles: #{num_uv_active_handles()}"
    puts "- UV active reqs: #{num_uv_active_reqs()}"
    puts "- @_handles (#{@_handles.size}):\n"
      @_handles.to_a[0..10].each {|k,v| puts "  - #{k}: #{v.inspect}"}
    puts "- @_procs (#{@_procs.size}):\n"
      @_procs.to_a[0..10].each {|k,v| puts "  - #{k}: #{v.inspect}"}
    puts "- @_next_tick_procs (#{@_next_tick_procs.size}):\n"
      @_next_tick_procs[0..10].each {|n| puts "  - #{n.inspect}"}
    puts "- @_call_from_other_thread_procs (#{@_call_from_other_thread_procs.size}):\n"
      @_call_from_other_thread_procs[0..10].each {|n| puts "  - #{n.inspect}"}
    puts
  end


  class << self
    private :num_uv_active_handles
    private :num_uv_active_reqs
  end

end
