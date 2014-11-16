#use version 1.9.3 (ruby1.9.3)
require_relative 'queue'

describe Queue do
  before(:each) do
    @queue = Queue.new
  end

  it "operates on the principle first in/first out" do
    @queue.enqueue( 1 )
    @queue.enqueue( 5 )

    @queue.dequeue.should eq(1)
    @queue.dequeue.should eq(5)
  end

  describe "#enqueue" do
    it "puts an element on queue " do
      @queue.enqueue( 1 )

      @queue.empty?.should be_false
      @queue.peek.should eq(1)
    end
  end

  describe "#peek" do
    it "returns nil on empty queue" do
      @queue.peek.should eq(nil)
    end

    it "returns first on queue without dequeueing it" do
      @queue.enqueue( 3 )

      @queue.peek.should eq(3)
      @queue.empty?.should be_false
    end
  end

  describe "#dequeue" do
    it "returns nil on empty queue" do
      @queue.dequeue.should eq(nil)
    end

    it "returns first on queue and removes it" do
      @queue.enqueue( 2 )

      @queue.dequeue.should eq(2)
      @queue.empty?.should be_true
    end
  end

  describe "#empty?" do
    it "is true on empty queue" do
      @queue.empty?.should be_true
    end

    it "is false when anything is enqueud" do
      @queue.enqueue( 7 )

      @queue.empty?.should be_false
    end
  end
end
