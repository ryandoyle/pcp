require 'pcp/pmapi'

module PCP
  class PMAPI
    class PmUnits

      def ==(other)
        self.class == other.class && \
        dimSpace == other.dimSpace && \
        dimTime == other.dimTime && \
        dimCount == other.dimCount && \
        scaleSpace == other.scaleSpace && \
        scaleTime == other.scaleTime && \
        scaleCount == other.scaleCount
      end

      def inspect
        "#<#{self.class.name}: dimSpace=#{dimSpace} dimTime=#{dimTime} dimCount=#{dimCount} scaleSpace=#{scaleSpace} scaleTime=#{scaleTime} scaleCount=#{scaleCount}>"
      end

    end
  end
end