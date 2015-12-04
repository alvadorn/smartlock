class LogsModifies < ActiveRecord::Migration
  def change
		add_column :logs, :account, :string, null: true
		add_column :logs, :password, :string, null: true
		add_column :logs, :token, :string, null: true
		remove_column :logs, :user_id
  end
end
