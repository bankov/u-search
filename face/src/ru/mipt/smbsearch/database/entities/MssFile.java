package ru.mipt.smbsearch.database.entities;

import java.io.Serializable;
import java.util.Date;
import java.util.List;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.NamedNativeQuery;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

//import ru.mipt.smbsearch.server.FindFilesServiceImpl;

/**
 * The persistent class for the mss_files database table.
 * 
 */
@Entity
@Table(
        name = "mss_files")
@NamedQueries({
        @NamedQuery(
                name = "getFileById", query = "select file from MssFile file where file.id = :id"),
        @NamedQuery(
                name = "getFileByServer",
                query = "select file from MssFile file where file.serverName like :server"),
        @NamedQuery(
                name = "getFileByNameAndServer",
                query = "select file from MssFile file where file.name like :name and file.serverName like :server") })
@NamedNativeQuery(
        name = "getFileByName", query = "select files.id ID, files.last_seen LAST_SEEN, files.name NAME, "
                + "files.server_name SERVER_NAME, files.file_path FILE_PATH "
                + "from mss_files files inner join full_text_search search "
                + "on search.id = files.id where match(text) against(? in boolean mode) >0 "
                + "ORDER BY MATCH(text) AGAINST (? IN BOOLEAN MODE) DESC", resultClass = MssFile.class)
public class MssFile implements Serializable {
    private static final long serialVersionUID = 1L;

    @Id
    @GeneratedValue(
            strategy = GenerationType.AUTO)
    @Column(
            name = "id")
    private int id;

    @Column(
            name = "file_path")
    private String filePath;

    @Column(
            name = "last_seen")
    @Temporal(TemporalType.DATE)
    private Date lastSeen;

    @Column(
            name = "name")
    private String name;

    @Column(
            name = "server_name")
    private String serverName;

    // bi-directional many-to-one association to MssParameter
    @OneToMany(
            mappedBy = "mssFile")
    private List<MssParameter> mssParameters;

    public MssFile() {
    }

    public int getId() {
        return this.id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getFilePath() {
        return this.filePath;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }

    public Date getLastSeen() {
        return this.lastSeen;
    }

    public void setLastSeen(Date lastSeen) {
        this.lastSeen = lastSeen;
    }

    public String getName() {
        return this.name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getServerName() {
        return this.serverName;
    }

    public void setServerName(String serverName) {
        this.serverName = serverName;
    }

    public List<MssParameter> getMssParameters() {
        return this.mssParameters;
    }

    public void setMssParameters(List<MssParameter> mssParameters) {
        this.mssParameters = mssParameters;
    }

    public MssParameter addMssParameter(MssParameter mssParameter) {
        getMssParameters().add(mssParameter);
        mssParameter.setMssFile(this);

        return mssParameter;
    }

    public MssParameter removeMssParameter(MssParameter mssParameter) {
        getMssParameters().remove(mssParameter);
        mssParameter.setMssFile(null);

        return mssParameter;
    }

    @Override
    public String toString(){
    	return String.format("%s at %s", filePath, serverName);
    }

}