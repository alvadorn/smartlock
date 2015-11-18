require 'test_helper'

class AuthenticateControllerTest < ActionController::TestCase
  test "should get init" do
    get :init
    assert_response :success
  end

end
