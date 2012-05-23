module AsyncEngine

  def self.call_from_other_thread pr=nil, &bl
    ensure_AE_is_ready_for_handles()

    _c_call_from_other_thread pr || bl
  end

  class << self
    private :_c_call_from_other_thread
  end

end