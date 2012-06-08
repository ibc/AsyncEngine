module AsyncEngine

  def self.call_from_other_thread pr=nil, &bl
    check_running()

    _c_call_from_other_thread pr || bl
  end

  class << self
    private :_c_call_from_other_thread
  end

end