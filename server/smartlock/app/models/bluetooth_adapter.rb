class BluetoothAdapter < ActiveRecord::Base
	belongs_to :user
	validates :token, presence: true, uniqueness: true
end
