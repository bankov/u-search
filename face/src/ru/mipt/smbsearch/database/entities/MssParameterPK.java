package ru.mipt.smbsearch.database.entities;

import java.io.Serializable;
import javax.persistence.*;

/**
 * The primary key class for the mss_parameters database table.
 */
@Embeddable
public class MssParameterPK implements Serializable {
	//default serial version id, required for serializable classes.
	private static final long serialVersionUID = 1L;

	@Column(name="attr_id")
	private int attrId;

	@Column(name="file_id")
	private int fileId;

	public MssParameterPK() {
	}
	public int getAttrId() {
		return this.attrId;
	}
	public void setAttrId(int attrId) {
		this.attrId = attrId;
	}
	public int getFileId() {
		return this.fileId;
	}
	public void setFileId(int fileId) {
		this.fileId = fileId;
	}

	public boolean equals(Object other) {
		if (this == other) {
			return true;
		}
		if (!(other instanceof MssParameterPK)) {
			return false;
		}
		MssParameterPK castOther = (MssParameterPK)other;
		return 
			(this.attrId == castOther.attrId)
			&& (this.fileId == castOther.fileId);
	}

	public int hashCode() {
		final int prime = 31;
		int hash = 17;
		hash = hash * prime + this.attrId;
		hash = hash * prime + this.fileId;
		
		return hash;
	}
}
