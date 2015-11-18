class CreateLogs < ActiveRecord::Migration
  def change
    create_table :logs do |t|
      t.boolean :bluetooth
      t.boolean :status
      t.integer :user_id, null: true

      t.timestamps null: false
    end
  end
end
