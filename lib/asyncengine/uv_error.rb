module AsyncEngine

  class Error < StandardError ; end

  class UvError < Error
    attr_reader :errno, :type, :description

    def initialize errno, type, description
      @errno = errno
      @type = type
      @description = description

      super "#{@type.inspect} (errno: #{@errno}), #{@description}"
    end
  end

end