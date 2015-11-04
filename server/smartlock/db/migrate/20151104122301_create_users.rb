class CreateUsers < ActiveRecord::Migration
  def change
    create_table :users do |t|
      t.string :name
      t.string :account
      t.string :password

      t.timestamps null: false
    end
  end
end
