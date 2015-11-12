require 'pcp_native'

module PCP
  class PMAPI
    class PmUnits

      def initialize(dimSpace, dimTime, dimCount, scaleSpace, scaleTime, scaleCount)
        self.dimSpace = dimSpace
        self.dimTime = dimTime
        self.dimCount = dimCount
        self.scaleSpace = scaleSpace
        self.scaleTime = scaleTime
        self.scaleCount = scaleCount
      end

      def to_h
        {
          :dimSpace => dimSpace,
          :dimTime  => dimTime,
          :dimCount => dimCount,
          :scaleSpace => scaleSpace,
          :scaleTime => scaleTime,
          :scaleCount => scaleCount,
        }
      end

      def ==(other)
        other.is_a?(PCP::PMAPI::PmUnits) && to_h == other.to_h
      end

      def inspect
        "#<#{self.class.name}: dimSpace=#{dimSpace} dimTime=#{dimTime} dimCount=#{dimCount} scaleSpace=#{scaleSpace} scaleTime=#{scaleTime} scaleCount=#{scaleCount}>"
      end

    end
  end
end