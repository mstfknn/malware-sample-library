using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Data.SQLite;
using System.Windows.Forms;
using System.Threading;

namespace Server
{
    public class DB
    {
        SQLiteConnection cnSQL;

        public DB(string path)
        {
			// cnSQL = new OleDbConnection("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + PathMDB(path));
            cnSQL = new SQLiteConnection("Version=3;Data Source=" + PathMDB(path) + ";");
            //cnSQL.Open();
			UpdateDB();
        }

        public DB(SQLiteConnection cn)
		{
			cnSQL = cn;
		}

		void UpdateDB()
		{
            Open();
            // if table bots exists, exit
            if (GetInt("SELECT count(*) FROM sqlite_master WHERE type='table' AND name='bots';") > 0)
            {
                return;
            }
            
            SQLiteCommand cmd = cnSQL.CreateCommand();
            string sql_command = "PRAGMA foreign_keys = off;"
                + "BEGIN TRANSACTION;"
                + "CREATE TABLE 'bots' ("
                + "'Id' INTEGER NOT NULL, 'UID' TEXT, 'IP' TEXT,"
                + "'FirstDate' DATETIME, 'LastDate' DATETIME, 'Info' TEXT,"
                + "'Comment' TEXT, 'IdGroup' INTEGER DEFAULT 0, PRIMARY KEY('Id'));"
                + "CREATE TABLE IF NOT EXISTS 'groups' ('Id' INTEGER NOT NULL, 'Name' TEXT, PRIMARY KEY('Id'));"
                + "COMMIT TRANSACTION;"
                + "PRAGMA foreign_keys = on;";
            cmd.CommandText = sql_command;
            try
            {
                cmd.ExecuteNonQuery();
            }
            catch (SQLiteException ex)
            {
                Console.WriteLine(ex.Message);
            }
		}

        //void UpdateToVer1()
        //{
        //    SendSQL("CREATE TABLE groups (Id COUNTER, Name TEXT(50))");
        //    SendSQL("CREATE INDEX PrimaryKey ON groups (Id) WITH PRIMARY");
        //}

		public static string PathMDB(string name)
		{
			if (name.IndexOf('\\') < 0 && name.IndexOf('/') < 0)
				return Application.StartupPath + '\\' + name;
			return name;
		}

        public SQLiteConnection CloneConnection()
		{
            return new SQLiteConnection(cnSQL.ConnectionString);
		}

		public DB Clone()
		{
			return new DB(CloneConnection());
		}

        public SQLiteDataAdapter GetDataAdapterSql(string sql)
		{
			return GetDataAdapterSql(sql);
		}

        public SQLiteDataAdapter GetDataAdapterSql(string sqlSelect, string sqlUpdate)
        {
            SQLiteCommand select = new SQLiteCommand(sqlSelect, cnSQL);
            SQLiteCommand selectUpdate = new SQLiteCommand(sqlUpdate, cnSQL);
            //выявляем параметры в запросе и создаем их в команде
            int p = 0;
			while (p < sqlSelect.Length)
            {
				p = sqlSelect.IndexOf('@', p);
                if (p < 0) break;
                int p2;
				for (p2 = ++p; p2 < sqlSelect.Length; p2++) //ищем конец имени поля 
					if (!char.IsLetterOrDigit(sqlSelect[p2])) break;
                select.Parameters.Add(new SQLiteParameter(sqlSelect.Substring(p, p2 - p), null));
            }
            SQLiteDataAdapter da = new SQLiteDataAdapter(selectUpdate);
            SQLiteCommandBuilder cb = new SQLiteCommandBuilder(da);
			cb.QuotePrefix = "[";
			cb.QuoteSuffix = "]";
			da.UpdateCommand = cb.GetUpdateCommand();
			da.InsertCommand = cb.GetInsertCommand();
			da.DeleteCommand = cb.GetDeleteCommand();
			da.SelectCommand = select;
			return da;
        }

        public void UpdateDataAdapter(SQLiteDataAdapter da)
		{
		}

        public DataTable GetTable(string sql)
        {
            SQLiteDataAdapter da = new SQLiteDataAdapter(sql, cnSQL);
            DataTable tbl = new DataTable();
            da.Fill(tbl);
            return tbl;
        }

        public DataTable GetTable(string sql, SQLiteParameter[] param)
        {
            SQLiteDataAdapter da = new SQLiteDataAdapter(sql, cnSQL);
            for (int i = 0; i < param.Length; i++)
                da.SelectCommand.Parameters.Add(param[i]);
            DataTable tbl = new DataTable();
            da.Fill(tbl);
            return tbl;
        }

		public void Open()
		{
			cnSQL.Open();
		}

		public void Close()
		{
			cnSQL.Close();
		}

		public object SendSQL(string sql)
		{
            SQLiteCommand cmd = new SQLiteCommand(sql, cnSQL);
			bool isOpen = false;
			if (cnSQL.State == ConnectionState.Open)
				isOpen = true;
			else
				cnSQL.Open();
			object ret = cmd.ExecuteScalar();
			if (!isOpen) cnSQL.Close();
			return ret;
		}

        public int GetInt(string sql)
        {
            DataTable tbl = GetTable(sql);
            if (tbl.Rows.Count > 0)
                return DB.Nz.GetInt(tbl.Rows[0][0]);
            return 0;
        }

        public SQLiteDataReader GetDataReader(string sql)
		{
			return GetDataReader(sql, cnSQL);
		}

        public SQLiteDataReader GetDataReader(string sql, SQLiteConnection cn)
		{
            SQLiteCommand cmd = new SQLiteCommand(sql, cn);
			return cmd.ExecuteReader();
		}

		public double GetDouble(string sql)
		{
			DataTable tbl = GetTable(sql);
			if (tbl.Rows.Count > 0)
				return DB.Nz.GetDouble(tbl.Rows[0][0]);
			return 0;
		}

		public string GetString(string sql)
		{
			DataTable tbl = GetTable(sql);
			if (tbl.Rows.Count > 0)
				return DB.Nz.GetString(tbl.Rows[0][0]);
			return string.Empty;
		}
		
		public int GetLastId()
        {
            //return GetInt("SELECT @@IDENTITY");
            return GetInt("select last_insert_rowid()");
        }

        public SQLiteConnection Connection
		{
			get { return cnSQL; }
		}

        static public class Nz
        {
            static public double GetDouble(object o)
            {
                if (o == null || Convert.IsDBNull(o))
                    return 0;
                return Convert.ToDouble(o);
            }

            static public decimal GetDecimal(object o)
            {
                if (o == null || Convert.IsDBNull(o))
                    return 0;
                return Convert.ToDecimal(o);
            }

            static public int GetInt(object o)
            {
                if (o == null || Convert.IsDBNull(o))
                    return 0;
                return Convert.ToInt32(o);
            }

            static public string GetString(object o)
            {
                if (o == null || Convert.IsDBNull(o))
                    return string.Empty;
                return Convert.ToString(o);
            }

            static public string GetDateString(object o, string format)
            {
                if (o == null || Convert.IsDBNull(o))
                    return string.Empty;
                return Convert.ToDateTime(o).ToString(format);
            }

			static public DateTime GetDateTime(object o)
			{
				if (o == null || Convert.IsDBNull(o))
					return DateTime.MinValue;
				return Convert.ToDateTime(o);
			}

            static public byte[] GetByteArray(object o)
            {
                if (o == null || Convert.IsDBNull(o))
                    return new byte[0];
                return (byte[])o;
            }
        }
    }
}
