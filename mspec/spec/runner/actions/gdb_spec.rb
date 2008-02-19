require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/actions/gdb'
require File.dirname(__FILE__) + '/../../../runner/mspec'
require File.dirname(__FILE__) + '/../../../runner/state'

describe GdbAction do
  before :each do
    MSpec.stub!(:read_tags).and_return([])
  end
  
  it "creates an MatchFilter with its tag and desc arguments" do
    filter = mock('action filter', :null_object => true)
    MatchFilter.should_receive(:new).with(nil, "some", "thing").and_return(filter)
    GdbAction.new ["tag", "key"], ["some", "thing"]
  end
end

describe GdbAction, "#before" do
  before :each do
    MSpec.stub!(:read_tags).and_return([])
    @state = SpecState.new "Catch#me", "if you can"
  end
  
  it "does not invoke the debugger if the description does not match" do
    Kernel.should_not_receive(:yield_gdb)
    action = GdbAction.new nil, "match"
    action.before @state
  end
  
  it "invokes the debugger if the description matches" do
    Kernel.should_receive(:yield_gdb).with(true)
    action = GdbAction.new nil, "can"
    action.before @state
  end
end

describe GdbAction, "#register" do
  before :each do
    MSpec.stub!(:read_tags).and_return([])
    @action = GdbAction.new nil, nil
  end
  
  it "registers itself with MSpec for the :before action" do
    MSpec.should_receive(:register).with(:before, @action)
    @action.register
  end
end

describe GdbAction, "#unregister" do
  before :each do
    MSpec.stub!(:read_tags).and_return([])
    @action = GdbAction.new nil, nil
  end
  
  it "unregisters itself with MSpec for the :before action" do
    MSpec.should_receive(:unregister).with(:before, @action)
    @action.unregister
  end
end