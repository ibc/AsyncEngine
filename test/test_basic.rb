require "ae_test_helper"


class TestBasic < AETest

  def test_01_AE_is_restartable
    str = ""

    assert_false AE.running?
    AE.run { assert_true AE.running? }
    assert_false AE.running?

    AE.run {}
    assert_false AE.running?

    AE.run { AE.add_timer(0.001) { assert_true AE.running? ; str << "a" } }
    assert_false AE.running?

    AE.run { AE.next_tick { str << "b" } }
    AE.run {}

    AE.run do
      char = "b"
      iteration = 0
      pt1 = AE::PeriodicTimer.new(0.001) do
        str << char.next!
        iteration += 1
        pt1.cancel if iteration == 4
      end
    end
    assert_false AE.running?

    assert_equal "abcdef", str
  end

  # TODO: Esto provoca un fallo al final de rake test...:
  # 11 tests, 61 assertions, 0 failures, 0 errors, 0 skips
  #
  # Test run options: --seed 34982
  # rake aborted!
  # Command failed with status (1): [/usr/bin/ruby1.9.1 -I"lib:test" -I"/var/li...]
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/file_utils.rb:53:in `block in create_shell_runner'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/file_utils.rb:45:in `call'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/file_utils.rb:45:in `sh'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/file_utils_ext.rb:39:in `sh'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/file_utils.rb:82:in `ruby'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/file_utils_ext.rb:39:in `ruby'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/testtask.rb:99:in `block (2 levels) in define'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/file_utils_ext.rb:60:in `verbose'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/testtask.rb:98:in `block in define'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/task.rb:205:in `call'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/task.rb:205:in `block in execute'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/task.rb:200:in `each'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/task.rb:200:in `execute'
  # /var/lib/gems/1.9.1/gems/rake-0.9.2.2/lib/rake/task.rb:158:in `block in invoke_with_call_chain'
  # /usr/lib/ruby/1.9.1/monitor.rb:201:in `mon_synchronize'
  #
  # TODO: Solo ocurre con AE, algo hay chungo en el tema de la captura de excepciones...
  def test_02_exception_handler
    AE.set_exception_handler {|e| assert e.is_a? ::StandardError }
    #AE.set_exception_handler {|e| puts "AE.exception_handler rescues #{e.class}: #{e}" }

    assert_respond_to AE.instance_variable_get(:@_exception_handler), :call

    assert_nothing_raised do
      AE.run { qweqwe }
      AE.run { AE.next_tick { uhhhh } }
      AE.run { AE.add_timer(0) { uhhhh } }
      AE.run { AE.next_tick { oppps } ; AE.add_timer(0.001) { uhhhh } }
      AE.run { AE.next_tick { ouchh } ; AE.add_timer(0.001) { LOL } }
    end

    assert_nothing_raised do
      AE.run do
        #AE.add_timer(0.001) { ohhhh1 }
        #AE::Timer.new(0.001) { ohhhh2 }
        #pt1 = AE::PeriodicTimer.new(0.001) { pt1.stop ; ohhhh3 }
      end
    end

    # Dissable the exception handler again.
    AE.unset_exception_handler

    assert_equal nil, AE.instance_variable_get(:@_exception_manager)
  end

  def test_03_stop_works_as_expected
    @var = false
    AE.run { AE.next_tick { @var = true } ; AE.stop }

    assert_false AE.running?
    assert_true AE.run {}
    assert_false @var

    @var = false
    AE.run { AE.stop ; AE.next_tick { @var = true } }

    assert_false AE.running?
    assert_true AE.run {}
    assert_false @var

    @var = false
    AE.run do
      AE.next_tick { AE.next_tick { @var = true  } }
      AE.next_tick { AE::Timer.new(0) { @var = true  } }
      AE.next_tick { AE.stop }
    end

    assert_false AE.running?
    assert_true AE.run {}
    assert_false @var
  end

  def test_04_run_in_run
    num = 0

    AE.run do
      num+=1
      AE.run { num+=1 }
      AE.run { AE.run { num+=1 } }
      AE.next_tick { num+=1 }
      AE.next_tick { AE.next_tick { num+=1 } }  # Won't occur.
      AE.next_tick { AE.stop }
      AE.next_tick { AE.next_tick { num+=1 } }  # Won't occur.
    end

    assert_equal 4, num
  end

  def test_05_lalala  # TODO: quitarlo...
    # So don't block in this simple case:
    AE.run do
      AE.next_tick { AE.next_tick { } }
      AE.next_tick { AE.stop }
    end

    assert_false AE.running?
    assert_true AE.run {}
  end

  def test_06_multiple_stop
    AE.run do
      AE.next_tick { AE.stop ; AE.stop }
      AE.next_tick { AE.next_tick { AE.stop } }
    end

    assert_false AE.running?
    assert_true AE.run {}
  end

end
