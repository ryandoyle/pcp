require 'pcp/pmapi'

module PCP
  class PMAPI
    class PmResult

      def inspect
        "#<#{self.class.name}: timestamp=#{timestamp} numpmid=#{numpmid} vset={vset}>"
      end

      def ==(other)
        self.class == other.class && \
        timestamp == other.timestamp && \
        numpmid == other.numpmid && \
        vset == other.vset
      end

    end
  end
end
