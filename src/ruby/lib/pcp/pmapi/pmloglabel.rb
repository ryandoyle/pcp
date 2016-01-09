require 'pcp/pmapi'

module PCP
  class PMAPI
    class PmLogLabel

      def inspect
        "#<#{self.class.name}: ll_magic=#{ll_magic} ll_pid=#{ll_pid} ll_start=#{ll_start} ll_hostname=#{ll_hostname} ll_tz=#{ll_tz}>"
      end

    end
  end
end