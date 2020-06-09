package ru.cleverhause.users.entity;

import lombok.Data;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.ManyToMany;
import javax.persistence.SequenceGenerator;
import javax.persistence.Table;
import java.io.Serializable;
import java.util.Set;

@Entity
@Data
@Table(name = "clever_schema.roles")
public class Role implements Serializable {

    public static final String DEF_ROLE = "ROLE_USER";

    @Id
    @GeneratedValue(strategy = GenerationType.SEQUENCE, generator = "SEQ_ID")
    @SequenceGenerator(name = "SEQ_ID", sequenceName = "users_id_seq", schema = "clever_schema")
    private Long id;

    @Column(name = "rolename")
    private String rolename;

    @ManyToMany(mappedBy = "roles")
    private Set<User> users;
}
