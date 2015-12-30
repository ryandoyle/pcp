require 'pcp/pmapi'

module PCP
  class PMAPI
    class PmValue

      def initialize(inst, value)
        self.inst = inst
        @value = value
      end

      def ==(other)
        self.class == other.class && \
        inst == other.inst && \
        value == other.value
      end

      def inspect
        "#<#{self.class.name}: inst=#{inst} value=#{value.inspect}>"
      end

    end
  end
end