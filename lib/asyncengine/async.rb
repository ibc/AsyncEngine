module AsyncEngine

  def self.call_from_other_thread block1=nil, &block2
    _c_call_from_other_thread block1 || block2
  end

  class << self
    private :_c_call_from_other_thread
  end

end