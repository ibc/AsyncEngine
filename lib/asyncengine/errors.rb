module AsyncEngine

  class Error < ::StandardError ; end

  class UvError < Error
    attr_reader :uv_errno, :uv_type, :uv_description

    def initialize uv_errno, uv_type, uv_description
      @uv_errno = uv_errno
      @uv_type = uv_type
      @uv_description = uv_description

      super "#{@uv_type.to_s}, #{@uv_description} (UV_ERRNO: #{@uv_errno})"
    end
  end

  class StopException < Error ; end


  UV_ERRNOS = {
    -1 => UvError.new(-1, :UNKNOWN, "unknown error"),
    0 => UvError.new(0, :OK, "success"),
    1 => UvError.new(1, :EOF, "end of file"),
    2 => UvError.new(2, :EADDRINFO, "getaddrinfo error"),
    3 => UvError.new(3, :EACCES, "permission denied"),
    4 => UvError.new(4, :EAGAIN, "no more processes"),
    5 => UvError.new(5, :EADDRINUSE, "address already in use"),
    6 => UvError.new(6, :EADDRNOTAVAIL, "address not available"),
    7 => UvError.new(7, :EAFNOSUPPORT, "address family not supported by protocol"),
    8 => UvError.new(8, :EALREADY, "operation already in progress"),
    9 => UvError.new(9, :EBADF, "bad file descriptor"),
    10 => UvError.new(10, :EBUSY, "resource busy or locked"),
    11 => UvError.new(11, :ECONNABORTED, "software caused connection abort"),
    12 => UvError.new(12, :ECONNREFUSED, "connection refused"),
    13 => UvError.new(13, :ECONNRESET, "connection reset by peer"),
    14 => UvError.new(14, :EDESTADDRREQ, "destination address required"),
    15 => UvError.new(15, :EFAULT, "bad address in system call argument"),
    16 => UvError.new(16, :EHOSTUNREACH, "host is unreachable"),
    17 => UvError.new(17, :EINTR, "interrupted system call"),
    18 => UvError.new(18, :EINVAL, "invalid argument"),
    19 => UvError.new(19, :EISCONN, "socket is already connected"),
    20 => UvError.new(20, :EMFILE, "too many open files"),
    21 => UvError.new(21, :EMSGSIZE, "message too long"),
    22 => UvError.new(22, :ENETDOWN, "network is down"),
    23 => UvError.new(23, :ENETUNREACH, "network is unreachable"),
    24 => UvError.new(24, :ENFILE, "file table overflow"),
    25 => UvError.new(25, :ENOBUFS, "no buffer space available"),
    26 => UvError.new(26, :ENOMEM, "not enough memory"),
    27 => UvError.new(27, :ENOTDIR, "not a directory"),
    28 => UvError.new(28, :EISDIR, "illegal operation on a directory"),
    29 => UvError.new(29, :ENONET, "machine is not on the network"),
    # NOTE: No error 30 in UV.
    31 => UvError.new(31, :ENOTCONN, "socket is not connected"),
    32 => UvError.new(32, :ENOTSOCK, "socket operation on non-socket"),
    33 => UvError.new(33, :ENOTSUP, "operation not supported on socket"),
    34 => UvError.new(34, :ENOENT, "no such file or directory"),
    35 => UvError.new(35, :ENOSYS, "function not implemented"),
    36 => UvError.new(36, :EPIPE, "broken pipe"),
    37 => UvError.new(37, :EPROTO, "protocol error"),
    38 => UvError.new(38, :EPROTONOSUPPORT, "protocol not supported"),
    39 => UvError.new(39, :EPROTOTYPE, "protocol wrong type for socket"),
    40 => UvError.new(40, :ETIMEDOUT, "connection timed out"),
    41 => UvError.new(41, :ECHARSET, ""), # TODO: description.
    42 => UvError.new(42, :EAIFAMNOSUPPORT, "address family not supported by protocol"),
    # NOTE: No error 43 in UV.
    44 => UvError.new(44, :EAISERVICE, ""), # TODO: description.
    45 => UvError.new(45, :EAISOCKTYPE, ""), # TODO: description.
    46 => UvError.new(46, :ESHUTDOWN, "cannot send after transport endpoint shutdown"),
    47 => UvError.new(47, :EEXIST, "file already exists"),
    48 => UvError.new(48, :ESRCH, "no such process"),
    49 => UvError.new(49, :ENAMETOOLONG, "name too long"),
    50 => UvError.new(50, :EPERM, "operation not permitted"),
    51 => UvError.new(51, :ELOOP, "too many symbolic links encountered"),
    52 => UvError.new(52, :EXDEV, "cross-device link not permitted"),
    53 => UvError.new(53, :ENOTEMPTY, "directory not empty"),
    54 => UvError.new(54, :ENOSPC, "no space left on device"),
    55 => UvError.new(55, :EIO, "i/o error"),
    56 => UvError.new(56, :EROFS, "read-only file system")
  }

end