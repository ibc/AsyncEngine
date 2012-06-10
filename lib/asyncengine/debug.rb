module AsyncEngine

    # TODO: for testing
  def self.ensure_no_handles
    raise AsyncEngine::Error, "num_uv_active_handles = #{num_uv_active_handles()} (> 1)"  unless num_uv_active_handles() <= 1
    raise AsyncEngine::Error, "@_handles not empty"  unless @_handles.empty?
    raise AsyncEngine::Error, "@_blocks not empty"  unless @_blocks.empty?
    raise AsyncEngine::Error, "@_next_ticks not empty"  unless @_next_ticks.empty?
    raise AsyncEngine::Error, "@_call_from_other_thread_procs not empty"  unless @_call_from_other_thread_procs.empty?
    @_call_from_other_thread_procs
  end

  # TODO: for testing
  def self.debug
    puts "\nDBG: AsyncEngine debug:"
    puts "- AE.running: #{running?}"
    puts "- UV active handles: #{num_uv_active_handles()}"
    puts "- UV active reqs: #{num_uv_active_reqs()}"
    puts "- @_handles (#{@_handles.size}):\n"
      @_handles.to_a[0..10].each {|k,v| puts "  - #{k}: #{v.inspect}"}
    puts "- @_blocks (#{@_blocks.size}):\n"
      @_blocks.to_a[0..10].each {|k,v| puts "  - #{k}: #{v.inspect}"}
    puts "- @_next_ticks (#{@_next_ticks.size}):\n"
      @_next_ticks[0..10].each {|n| puts "  - #{n.inspect}"}
    puts "- @_call_from_other_thread_procs (#{@_call_from_other_thread_procs.size}):\n"
      @_call_from_other_thread_procs[0..10].each {|n| puts "  - #{n.inspect}"}
    puts
  end

  class << self
    private :num_uv_active_handles
    private :num_uv_active_reqs
  end

end
