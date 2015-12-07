require 'pcp_native'

module PCP
  class PMAPI
    class PmValue

      def initialize(inst, value)
        self.inst = inst
        @value = value
      end

      attr_accessor :value

      def ==(other)
        inst == other.inst && value == other.value
      end

      def inspect
        "#<#{self.class.name}: inst=#{inst} value=#{value}>"
      end

    end
  end
end