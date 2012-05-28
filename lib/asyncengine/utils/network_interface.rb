module AsyncEngine::Utils

  class NetworkInterface
    attr_reader :name, :ip_type, :ip

    def internal?
      @is_internal
    end

    alias :orig_to_s :to_s
    def to_s
      "#{self.orig_to_s} (name: #{@name}, internal?: #{@is_internal.inspect}, ip: #{@ip})"
    end
    alias :inspect :to_s
  end

end